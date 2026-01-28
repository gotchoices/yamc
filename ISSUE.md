# Refactor: Split local vs host cache, make `init` optional when already configured

## Summary

Today `yamc` treats `~/.yamc/<host>.env` as a hard prerequisite for running any remote module. This makes **`yamc init` effectively required on every local machine**, even when:

- the remote host was already initialized from some other machine, and
- the current machine already has working SSH access to the host.

This issue proposes a refactor to:

- **Segregate local vs host cached settings** (`yamc.env` vs `<host>.env`)
- Make YAMC **self-heal missing cache** at runtime (especially local cache)
- Keep `yamc init` as the default/explicit onboarding path, but **not required** when both ends are already configured.

## Decisions (record as we go)

- **No migration required**: we do not need to migrate existing `~/.yamc/*.env` contents. A machine with prior state (e.g., `aries`) should self-heal by regenerating missing/deficient cache files as needed.
- **Env file execution model**: we are OK with `yamc` using `source` on `yamc.env` files (no strict parsing/validation), assuming `yamc` itself only writes `var=value` pairs.
- **Meaning of `YAMC_USER` in `~/.yamc/<host>.env`**: `YAMC_USER` represents the “regular / bootstrap” SSH user (non-root, sudo-capable) for that host. We should **not** automatically write `YAMC_USER=root` into the host cache. Root usage should remain explicit (via `-u root` and/or `yamc/<module>/yamc.env` defaults).
- **No implicit init/repair**: if a requested module cannot run for any reason, YAMC should report the error and exit. YAMC should only perform init/bootstrapping steps when explicitly instructed (i.e., `yamc init`).
- **No auto-fix side effects**: even if YAMC can recognize specific failure modes (e.g., host key mismatch after rebuild), it should still only print an actionable error/hint and exit (no automatic modification of `known_hosts`, no implicit re-init).
- **Host cache creation**: `~/.yamc/<host>.env` should only be created when we have a non-root `YAMC_USER` value to write. If `YAMC_USER` is the only host-cached value, we should not create an empty `<host>.env`. If we later add additional host-cached values, this decision can be revisited.
- **Propagation of env values**: `yamc` should load (`source`) env files for its own use, and `.loc` scripts should see those values via inheritance. We should **not automatically export/cache-pass these values to remote scripts**. If a remote script needs a value, the `.loc` script can explicitly pass it using `-e var=value` (or equivalent future mechanism).

Non-goals:

- Removing interactive bootstrapping entirely (some setups still require sudo/password prompts).
- Changing the module interface (`MOD_DIR`, `MOD_TMP`, `RES_DIR`, etc.).

---

## Problem statements (with scenarios)

### Scenario A: cross-machine usage

- Run `yamc init` from machine A (e.g. `aries`) against host `mp14`
- Later run a YAMC module against `mp14` from machine B (e.g. a laptop)
- Machine B already has SSH access, and `mp14` already has `sshfs` installed from earlier init
- **Current behavior**: machine B fails because `~/.yamc/mp14.env` is missing locally
- **Desired behavior**: proceed without requiring `yamc init` on machine B

### Scenario B: host rebuild / lost key

- `~/.yamc/<host>.env` exists locally
- The remote host is rebuilt or loses authorized keys / packages
- **Desired behavior**: YAMC reports the failure clearly and exits (no auto-fix). The user can explicitly rerun `yamc init` (and/or other modules) to restore access and prerequisites.

---

## Proposed model: caches (local + host) and optional module metadata

### 1) Local cache: `~/.yamc/yamc.env`

Purpose: cache discoveries about **the local machine** (not host-specific).

Candidate contents:

- `YAMC_SFTP_SERVER=/path/to/sftp-server` (discovered)
- `YAMC_LOCAL_CACHE_VERSION=1`
- (optional) discovery timestamp(s)

Policy:

- Generated/updated **lazily**: any time it’s missing or deficient.
- Must be safe to overwrite or repair.

### 2) Host cache: `~/.yamc/<host>.env`

Purpose: store **host-specific preferences** (and only a small set of stable host facts, if needed).

Candidate contents (keep minimal):

- `YAMC_USER=<preferred SSH user for this host>` (optional; default connection user)
  - Initially: the `-u <user>` value (first use) when the cache is created
- `YAMC_HOST_CANONICAL=<last known canonical target>` (optional)
- (optional) last successful connect timestamp / last known OS ID (diagnostics only)

Policy:

- Missing file should **not** block attempts to run modules.
- If present, it can provide defaults (e.g., user), but should not be treated as proof of readiness.

---

## Optional enhancement: module-level defaults (`<module>/yamc.env`)

In practice, the SSH user (`-u`, `YAMC_USER`) is often determined by the module being run:

- Some modules should run as the “regular” bootstrap user (non-root, sudo-capable)
- Many modules should run as `root` (especially after `ssh2user`)

To reduce repeated `-u root`, we may support an optional per-module metadata file:

- Location: `yamc/<module>/yamc.env`
- Behavior: if present, it is sourced locally by the `yamc` runner to provide *default preferences* for the module, which can still be overridden.

Open question (to decide during implementation/testing): the variable name(s) and semantics. Candidates include:

- `YAMC_USER=<user>` (module supplies a default SSH user directly)
- `PREFERS_ROOT=1` (module expresses intent, runner chooses how to apply it)

Note: `yamc.env` may later grow to include other useful module-level settings as new features are added.

### Alternate approach: lightweight hint in `setup.loc`

Another possible pattern is to place a simple setting near the top of `<module>/setup.loc` (or `edit.loc`, etc.) to express a preference for root execution.

This needs careful consideration because `.loc` scripts currently run at a specific point in the flow, and we still need to define how such a hint should interact with:

- explicit `-u <user>` on the command line (CLI should win)
- host defaults from `~/.yamc/<host>.env`

We can keep this as an exploration item and decide after prototype/testing.

---

## Proposed behavior changes

### Default UX remains

`yamc init -h <host> -u <user>` stays the recommended/explicit onboarding step.

### Runtime preflight replaces “env file gate”

On module execution:

- Ensure `~/.yamc/yamc.env` exists and contains valid local prerequisites (e.g., `sftp-server` path)
- Attempt SSH connectivity with the resolved user/host
- If SSH is OK, proceed with module run (even if `~/.yamc/<host>.env` is missing)

If SSH is not OK:

- Diagnose failure class and choose a fallback path (always exits; no auto-fix):
  - network/DNS failure → report and stop
  - host key mismatch (rebuilt host) → report and stop (may include a targeted hint)
  - auth failure → report and stop (may include “run `yamc init ...`” guidance)

### “Init posture” (bootstrap/repair)

When in init posture, YAMC may attempt (possibly interactively):

- ensure SSH keypair exists locally
- install key to remote (`ssh-copy-id` or equivalent)
- ensure remote `sshfs` is installed (sudo)

Important: this posture may require a TTY and user interaction; it should be entered deliberately or with clear messaging.

---

## Checklist (implementation plan)

### Phase 0: scope confirmation (already decided)

- [x] No migration required
- [x] `source` env files (no strict parsing)
- [x] No implicit init/repair; no auto-fix side effects
- [x] `~/.yamc/<host>.env` stores only non-root `YAMC_USER` (create only when we have something to write)
- [x] Env values available to `yamc` + `.loc`, not automatically passed to remote scripts

### Phase 1: local cache (`yamc.env`)

- [ ] Add helper to read/write `~/.yamc/yamc.env` (local cache)
- [ ] Add `ensure_local_cache()` (self-heal):
  - [ ] detect `sftp-server` path robustly
  - [ ] validate cached value is executable
  - [ ] refresh cache if missing/invalid
- [ ] Update module execution path to use local cache for local settings (e.g., `YAMC_SFTP_SERVER`)

### Phase 2: host cache becomes optional

- [ ] Update execution path so missing `~/.yamc/<host>.env` does **not** block running
- [ ] If present, use it to supply default `YAMC_USER` (SSH login user), but allow precedence rules to override
- [ ] Ensure behavior is correct when `<host>.env` exists but is stale (no “hard trust”)
- [ ] Create `~/.yamc/<host>.env` on first successful run that has a non-root `YAMC_USER` to write

### Phase 3: module metadata (optional)

- [ ] Support `yamc/<module>/yamc.env` as an optional default source (primarily for `YAMC_USER` / permission preference)
- [ ] Confirm precedence interaction: `-u` > `module/yamc.env` > `~/.yamc/<host>.env` > ambient `YAMC_USER`

### Phase 4: docs + tests

- [ ] Update `yamc/README.md`:
  - [ ] explain local vs host cache
  - [ ] explain when `init` is required vs optional
- [ ] Extend `yamc/test` module to validate:
  - [ ] no host cache file present still runs when SSH works
  - [ ] local cache regeneration works
  - [ ] error output remains “report and exit” (no implicit init)

---

## Acceptance criteria

- [ ] On a fresh local machine with working SSH access to an already-initialized host:
  - [ ] `yamc -h <host> <module>` works without running `yamc init`
- [ ] If `~/.yamc/yamc.env` is missing or contains a bad `sftp-server` path:
  - [ ] YAMC repairs it automatically and proceeds
- [ ] If SSH fails, YAMC emits an actionable diagnosis:
  - [ ] distinguishes network/DNS vs host key mismatch vs auth failure (best-effort)
- [ ] Explicit `yamc init` still works as the primary onboarding/repair command

---

## Precedence: choosing the SSH user (`YAMC_USER`)

Proposed precedence for the username used to connect to the remote host:

1. `-u <user>` on the command line (highest priority)
2. `yamc/<module>/yamc.env` (module defaults; optional)
3. `~/.yamc/<host>.env` (host defaults; optional)
4. `YAMC_USER` from the user’s environment (ambient default; lowest priority)

This ordering is intended to let modules/hosts provide sensible defaults without being unexpectedly overridden by a stray exported `YAMC_USER`, while still allowing the CLI to override everything.

---

## Notes / open questions

- Do we want a `yamc doctor` / `yamc check` command to run preflight without running a module?


