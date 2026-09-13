# CP53 configuration mapping

Date: 13 September 2026

- Replaced regex extraction with vendored Jsonic++ at commit `88e4736`.
- Strict duplicate rejection, JSONC comments/trailing commas and structured
  parse locations are selected explicitly.
- Maps inherited `compilerOptions`, `files`, `include`, `exclude`, `baseUrl`,
  `paths`, `rootDirs` and `moduleResolution` through typed JSON values.
- Detects inheritance cycles and resolves inherited paths relative to the file
  that declares them.
- The program graph consumes path aliases, base URL and virtual root directories
  during dependency discovery and type propagation.
- Focused tests prove JSONC inheritance, wildcard alias resolution, cross-file
  type checking and cycle rejection.

CP54 is next. Declaration inputs already emit no JavaScript, but their frontend
must gain the ambient module/value/type/class surface required by the trials.
