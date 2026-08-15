function id42<T>(x: T): T { return x; }
class Base42<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child42 extends Base42<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E42 { A, B = 3, C, Label = "label" }
const x42: number = id42<number>(42);
const c42 = new Child42("n", x42);
console.log(c42.name, E42.A, E42[3]);
