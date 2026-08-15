function id33<T>(x: T): T { return x; }
class Base33<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child33 extends Base33<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E33 { A, B = 3, C, Label = "label" }
const x33: number = id33<number>(33);
const c33 = new Child33("n", x33);
console.log(c33.name, E33.A, E33[3]);
