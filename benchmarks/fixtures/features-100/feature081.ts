function id81<T>(x: T): T { return x; }
class Base81<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child81 extends Base81<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E81 { A, B = 3, C, Label = "label" }
const x81: number = id81<number>(81);
const c81 = new Child81("n", x81);
console.log(c81.name, E81.A, E81[3]);
