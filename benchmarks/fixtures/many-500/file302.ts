interface Item302 { id: number; name: string; }
type Key302 = string | number;
export function make302(id: number, name: string): Item302 {
  const value: Item302 = {id, name};
  return value as Item302;
}
export const item302: Item302 = make302(302, "item-302");
