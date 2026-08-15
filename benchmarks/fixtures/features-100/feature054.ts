function id54<T>(x: T): T { return x; }
class Base54<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child54 extends Base54<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E54 { A, B = 3, C, Label = "label" }
const x54: number = id54<number>(54);
const c54 = new Child54("n", x54);
console.log(c54.name, E54.A, E54[3]);
