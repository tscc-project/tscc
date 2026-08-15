function id85<T>(x: T): T { return x; }
class Base85<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child85 extends Base85<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E85 { A, B = 3, C, Label = "label" }
const x85: number = id85<number>(85);
const c85 = new Child85("n", x85);
console.log(c85.name, E85.A, E85[3]);
