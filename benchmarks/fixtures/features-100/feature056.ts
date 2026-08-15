function id56<T>(x: T): T { return x; }
class Base56<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child56 extends Base56<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E56 { A, B = 3, C, Label = "label" }
const x56: number = id56<number>(56);
const c56 = new Child56("n", x56);
console.log(c56.name, E56.A, E56[3]);
