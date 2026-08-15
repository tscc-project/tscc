interface Item55 { id: number; name: string; }
type Key55 = string | number;
export function make55(id: number, name: string): Item55 {
  const value: Item55 = {id, name};
  return value as Item55;
}
export const item55: Item55 = make55(55, "item-55");
