function id95<T>(x: T): T { return x; }
class Base95<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child95 extends Base95<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E95 { A, B = 3, C, Label = "label" }
const x95: number = id95<number>(95);
const c95 = new Child95("n", x95);
console.log(c95.name, E95.A, E95[3]);
