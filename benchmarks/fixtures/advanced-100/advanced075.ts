namespace N75 {
  export const base: number = 75;
  export function add(x: number): number { return base + x; }
}
function over75(x: number): number;
function over75(x: string): string;
function over75(x: number | string): number | string { return x; }
abstract class A75 { abstract f(x: number): number; }
class B75 extends A75 { override f(x: number): number { return x + 1; } }
enum E75 { A = 5 / 2, B, C = 2 ** 3, D }
const angle75 = <number>(75 + 1);
const check75 = {x: angle75} satisfies {x: number};
console.log(N75.add(1), over75(75), new B75().f(1), E75.B, E75.D, check75.x);
