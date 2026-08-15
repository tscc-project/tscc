function id58<T>(x: T): T { return x; }
class Base58<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child58 extends Base58<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E58 { A, B = 3, C, Label = "label" }
const x58: number = id58<number>(58);
const c58 = new Child58("n", x58);
console.log(c58.name, E58.A, E58[3]);
