namespace N36 {
  export const base: number = 36;
  export function add(x: number): number { return base + x; }
}
function over36(x: number): number;
function over36(x: string): string;
function over36(x: number | string): number | string { return x; }
abstract class A36 { abstract f(x: number): number; }
class B36 extends A36 { override f(x: number): number { return x + 1; } }
enum E36 { A = 5 / 2, B, C = 2 ** 3, D }
const angle36 = <number>(36 + 1);
const check36 = {x: angle36} satisfies {x: number};
console.log(N36.add(1), over36(36), new B36().f(1), E36.B, E36.D, check36.x);
