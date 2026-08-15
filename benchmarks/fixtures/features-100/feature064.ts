function id64<T>(x: T): T { return x; }
class Base64<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child64 extends Base64<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E64 { A, B = 3, C, Label = "label" }
const x64: number = id64<number>(64);
const c64 = new Child64("n", x64);
console.log(c64.name, E64.A, E64[3]);
