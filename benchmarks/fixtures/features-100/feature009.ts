function id9<T>(x: T): T { return x; }
class Base9<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child9 extends Base9<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E9 { A, B = 3, C, Label = "label" }
const x9: number = id9<number>(9);
const c9 = new Child9("n", x9);
console.log(c9.name, E9.A, E9[3]);
