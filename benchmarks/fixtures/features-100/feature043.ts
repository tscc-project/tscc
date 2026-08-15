function id43<T>(x: T): T { return x; }
class Base43<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child43 extends Base43<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E43 { A, B = 3, C, Label = "label" }
const x43: number = id43<number>(43);
const c43 = new Child43("n", x43);
console.log(c43.name, E43.A, E43[3]);
