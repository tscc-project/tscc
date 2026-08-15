function id83<T>(x: T): T { return x; }
class Base83<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child83 extends Base83<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E83 { A, B = 3, C, Label = "label" }
const x83: number = id83<number>(83);
const c83 = new Child83("n", x83);
console.log(c83.name, E83.A, E83[3]);
