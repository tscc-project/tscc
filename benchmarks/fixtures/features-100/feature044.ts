function id44<T>(x: T): T { return x; }
class Base44<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child44 extends Base44<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E44 { A, B = 3, C, Label = "label" }
const x44: number = id44<number>(44);
const c44 = new Child44("n", x44);
console.log(c44.name, E44.A, E44[3]);
