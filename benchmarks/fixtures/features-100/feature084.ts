function id84<T>(x: T): T { return x; }
class Base84<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child84 extends Base84<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E84 { A, B = 3, C, Label = "label" }
const x84: number = id84<number>(84);
const c84 = new Child84("n", x84);
console.log(c84.name, E84.A, E84[3]);
