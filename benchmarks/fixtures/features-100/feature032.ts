function id32<T>(x: T): T { return x; }
class Base32<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child32 extends Base32<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E32 { A, B = 3, C, Label = "label" }
const x32: number = id32<number>(32);
const c32 = new Child32("n", x32);
console.log(c32.name, E32.A, E32[3]);
