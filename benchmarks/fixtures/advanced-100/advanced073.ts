namespace N73 {
  export const base: number = 73;
  export function add(x: number): number { return base + x; }
}
function over73(x: number): number;
function over73(x: string): string;
function over73(x: number | string): number | string { return x; }
abstract class A73 { abstract f(x: number): number; }
class B73 extends A73 { override f(x: number): number { return x + 1; } }
enum E73 { A = 5 / 2, B, C = 2 ** 3, D }
const angle73 = <number>(73 + 1);
const check73 = {x: angle73} satisfies {x: number};
console.log(N73.add(1), over73(73), new B73().f(1), E73.B, E73.D, check73.x);
