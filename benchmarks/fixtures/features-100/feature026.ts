function id26<T>(x: T): T { return x; }
class Base26<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child26 extends Base26<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E26 { A, B = 3, C, Label = "label" }
const x26: number = id26<number>(26);
const c26 = new Child26("n", x26);
console.log(c26.name, E26.A, E26[3]);
