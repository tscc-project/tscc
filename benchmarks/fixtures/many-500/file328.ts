interface Item328 { id: number; name: string; }
type Key328 = string | number;
export function make328(id: number, name: string): Item328 {
  const value: Item328 = {id, name};
  return value as Item328;
}
export const item328: Item328 = make328(328, "item-328");
