function id61<T>(x: T): T { return x; }
class Base61<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child61 extends Base61<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E61 { A, B = 3, C, Label = "label" }
const x61: number = id61<number>(61);
const c61 = new Child61("n", x61);
console.log(c61.name, E61.A, E61[3]);
