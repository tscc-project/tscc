function id46<T>(x: T): T { return x; }
class Base46<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child46 extends Base46<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E46 { A, B = 3, C, Label = "label" }
const x46: number = id46<number>(46);
const c46 = new Child46("n", x46);
console.log(c46.name, E46.A, E46[3]);
