#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'find "$TMP" -type f -delete; find "$TMP" -depth -type d -empty -delete' EXIT

cat >"$TMP/good.ts" <<'TS'
interface Named { name:string; describe():string; }
abstract class Base {
  protected code:number;
  constructor(code:number) { this.code=code; }
  abstract describe():string;
  getCode():number { return this.code; }
}
class Item extends Base implements Named {
  constructor(public readonly name:string,code:number) { super(code); }
  override describe():string { return this.name; }
}
const item:Item=new Item("answer",42);
const inherited:number=item.getCode();
const description:string=item.describe();
class Parser {
  constructor(value:number);
  constructor(value:string);
  constructor(value:number|string) {}
  parse(value:number):number;
  parse(value:string):string;
  parse(value:unknown):unknown { return value; }
}
const parser:Parser=new Parser(1);
const parsedNumber:number=parser.parse(1);
const parsedString:string=parser.parse("x");
class GenericBox<T extends string> { constructor(public value:T) {} }
const generic:GenericBox<string>=new GenericBox("value");
const genericValue:string=generic.value;
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/good.ts"

check_error() {
  local name=$1 expected=$2
  if "$ROOT/tscc" --pretty false --noEmit "$TMP/$name.ts" >"$TMP/$name.out" 2>"$TMP/$name.err"; then
    echo "$name was unexpectedly accepted" >&2; exit 1
  fi
  grep -Fq "$expected" "$TMP/$name.err"
}

cat >"$TMP/interface.ts" <<'TS'
interface Named { name:string; }
class Missing implements Named {}
TS
check_error interface "incorrectly implements interface 'Named'"

cat >"$TMP/override.ts" <<'TS'
class Base { value():number { return 1; } }
class Child extends Base { override value():string { return "x"; } }
TS
check_error override "is not assignable to the same property in base class 'Base'"

cat >"$TMP/override-missing.ts" <<'TS'
class Base {}
class Child extends Base { override value():number { return 1; } }
TS
check_error override-missing "cannot have an 'override' modifier"

cat >"$TMP/abstract.ts" <<'TS'
abstract class Base { abstract value():number; }
const base:Base=new Base();
TS
check_error abstract "Cannot create an instance of an abstract class."

cat >"$TMP/abstract-member.ts" <<'TS'
abstract class Base { abstract value():number; }
class Child extends Base {}
TS
check_error abstract-member "does not implement inherited abstract member 'value'"

cat >"$TMP/access.ts" <<'TS'
class Base { private secret:number=1; protected code:number=2; }
class Child extends Base { read():number { return this.code; } }
const child:Child=new Child();
const secret:number=child.secret;
const code:number=child.code;
TS
check_error access "Property 'secret' is private"
grep -Fq "Property 'code' is protected" "$TMP/access.err"

cat >"$TMP/readonly.ts" <<'TS'
class Item { constructor(public readonly name:string) {} }
const item:Item=new Item("x");
item.name="y";
TS
check_error readonly "Cannot assign to 'name' because it is a read-only property."

cat >"$TMP/super.ts" <<'TS'
class Base { constructor(value:number) {} }
class Child extends Base { constructor() { super("wrong"); } }
TS
check_error super "not assignable to parameter of type 'number'"

cat >"$TMP/cycle.ts" <<'TS'
class First extends Second {}
class Second extends First {}
TS
check_error cycle "Class inheritance cycle involving"

cat >"$TMP/member-overload.ts" <<'TS'
class Parser {
  parse(value:number):number;
  parse(value:string):string;
  parse(value:unknown):unknown { return value; }
}
const parser:Parser=new Parser();
parser.parse(true);
TS
check_error member-overload "No overload matches this call."

cat >"$TMP/constructor-overload.ts" <<'TS'
class Box {
  constructor(value:number);
  constructor(value:string);
  constructor(value:number|string) {}
}
const box:Box=new Box(true);
TS
check_error constructor-overload "No overload matches this call."

cat >"$TMP/generic-class.ts" <<'TS'
class Box<T extends string> { constructor(public value:T) {} }
const box:Box<string>=new Box(42);
TS
check_error generic-class "Invalid or unresolved type arguments for class 'Box'."

printf 'tscc deterministic class relationships passed\n'
