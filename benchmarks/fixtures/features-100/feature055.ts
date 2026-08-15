function id55<T>(x: T): T { return x; }
class Base55<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child55 extends Base55<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E55 { A, B = 3, C, Label = "label" }
const x55: number = id55<number>(55);
const c55 = new Child55("n", x55);
console.log(c55.name, E55.A, E55[3]);
