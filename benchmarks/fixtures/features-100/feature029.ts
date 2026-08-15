function id29<T>(x: T): T { return x; }
class Base29<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child29 extends Base29<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E29 { A, B = 3, C, Label = "label" }
const x29: number = id29<number>(29);
const c29 = new Child29("n", x29);
console.log(c29.name, E29.A, E29[3]);
