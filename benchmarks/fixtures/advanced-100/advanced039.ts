namespace N39 {
  export const base: number = 39;
  export function add(x: number): number { return base + x; }
}
function over39(x: number): number;
function over39(x: string): string;
function over39(x: number | string): number | string { return x; }
abstract class A39 { abstract f(x: number): number; }
class B39 extends A39 { override f(x: number): number { return x + 1; } }
enum E39 { A = 5 / 2, B, C = 2 ** 3, D }
const angle39 = <number>(39 + 1);
const check39 = {x: angle39} satisfies {x: number};
console.log(N39.add(1), over39(39), new B39().f(1), E39.B, E39.D, check39.x);
