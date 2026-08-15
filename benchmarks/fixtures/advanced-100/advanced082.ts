namespace N82 {
  export const base: number = 82;
  export function add(x: number): number { return base + x; }
}
function over82(x: number): number;
function over82(x: string): string;
function over82(x: number | string): number | string { return x; }
abstract class A82 { abstract f(x: number): number; }
class B82 extends A82 { override f(x: number): number { return x + 1; } }
enum E82 { A = 5 / 2, B, C = 2 ** 3, D }
const angle82 = <number>(82 + 1);
const check82 = {x: angle82} satisfies {x: number};
console.log(N82.add(1), over82(82), new B82().f(1), E82.B, E82.D, check82.x);
