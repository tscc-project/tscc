interface Item138 { id: number; name: string; }
type Key138 = string | number;
export function make138(id: number, name: string): Item138 {
  const value: Item138 = {id, name};
  return value as Item138;
}
export const item138: Item138 = make138(138, "item-138");
