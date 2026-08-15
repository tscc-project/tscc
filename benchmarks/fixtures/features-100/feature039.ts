function id39<T>(x: T): T { return x; }
class Base39<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child39 extends Base39<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E39 { A, B = 3, C, Label = "label" }
const x39: number = id39<number>(39);
const c39 = new Child39("n", x39);
console.log(c39.name, E39.A, E39[3]);
