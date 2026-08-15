interface Item127 { id: number; name: string; }
type Key127 = string | number;
export function make127(id: number, name: string): Item127 {
  const value: Item127 = {id, name};
  return value as Item127;
}
export const item127: Item127 = make127(127, "item-127");
