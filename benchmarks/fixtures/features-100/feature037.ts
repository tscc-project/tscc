function id37<T>(x: T): T { return x; }
class Base37<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child37 extends Base37<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E37 { A, B = 3, C, Label = "label" }
const x37: number = id37<number>(37);
const c37 = new Child37("n", x37);
console.log(c37.name, E37.A, E37[3]);
