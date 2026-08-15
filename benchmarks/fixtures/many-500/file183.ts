interface Item183 { id: number; name: string; }
type Key183 = string | number;
export function make183(id: number, name: string): Item183 {
  const value: Item183 = {id, name};
  return value as Item183;
}
export const item183: Item183 = make183(183, "item-183");
