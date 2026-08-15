function id70<T>(x: T): T { return x; }
class Base70<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child70 extends Base70<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E70 { A, B = 3, C, Label = "label" }
const x70: number = id70<number>(70);
const c70 = new Child70("n", x70);
console.log(c70.name, E70.A, E70[3]);
