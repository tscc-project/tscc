interface Item282 { id: number; name: string; }
type Key282 = string | number;
export function make282(id: number, name: string): Item282 {
  const value: Item282 = {id, name};
  return value as Item282;
}
export const item282: Item282 = make282(282, "item-282");
