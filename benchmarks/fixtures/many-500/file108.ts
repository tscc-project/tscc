interface Item108 { id: number; name: string; }
type Key108 = string | number;
export function make108(id: number, name: string): Item108 {
  const value: Item108 = {id, name};
  return value as Item108;
}
export const item108: Item108 = make108(108, "item-108");
