function id24<T>(x: T): T { return x; }
class Base24<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child24 extends Base24<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E24 { A, B = 3, C, Label = "label" }
const x24: number = id24<number>(24);
const c24 = new Child24("n", x24);
console.log(c24.name, E24.A, E24[3]);
