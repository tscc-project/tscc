function id7<T>(x: T): T { return x; }
class Base7<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child7 extends Base7<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E7 { A, B = 3, C, Label = "label" }
const x7: number = id7<number>(7);
const c7 = new Child7("n", x7);
console.log(c7.name, E7.A, E7[3]);
