namespace N49 {
  export const base: number = 49;
  export function add(x: number): number { return base + x; }
}
function over49(x: number): number;
function over49(x: string): string;
function over49(x: number | string): number | string { return x; }
abstract class A49 { abstract f(x: number): number; }
class B49 extends A49 { override f(x: number): number { return x + 1; } }
enum E49 { A = 5 / 2, B, C = 2 ** 3, D }
const angle49 = <number>(49 + 1);
const check49 = {x: angle49} satisfies {x: number};
console.log(N49.add(1), over49(49), new B49().f(1), E49.B, E49.D, check49.x);
