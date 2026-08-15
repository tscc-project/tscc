function id31<T>(x: T): T { return x; }
class Base31<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child31 extends Base31<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E31 { A, B = 3, C, Label = "label" }
const x31: number = id31<number>(31);
const c31 = new Child31("n", x31);
console.log(c31.name, E31.A, E31[3]);
