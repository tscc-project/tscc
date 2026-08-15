interface Item178 { id: number; name: string; }
type Key178 = string | number;
export function make178(id: number, name: string): Item178 {
  const value: Item178 = {id, name};
  return value as Item178;
}
export const item178: Item178 = make178(178, "item-178");
