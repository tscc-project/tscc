function id66<T>(x: T): T { return x; }
class Base66<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child66 extends Base66<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E66 { A, B = 3, C, Label = "label" }
const x66: number = id66<number>(66);
const c66 = new Child66("n", x66);
console.log(c66.name, E66.A, E66[3]);
