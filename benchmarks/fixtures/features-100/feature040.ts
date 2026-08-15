function id40<T>(x: T): T { return x; }
class Base40<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child40 extends Base40<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E40 { A, B = 3, C, Label = "label" }
const x40: number = id40<number>(40);
const c40 = new Child40("n", x40);
console.log(c40.name, E40.A, E40[3]);
