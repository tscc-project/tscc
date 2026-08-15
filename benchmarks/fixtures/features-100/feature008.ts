function id8<T>(x: T): T { return x; }
class Base8<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child8 extends Base8<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E8 { A, B = 3, C, Label = "label" }
const x8: number = id8<number>(8);
const c8 = new Child8("n", x8);
console.log(c8.name, E8.A, E8[3]);
