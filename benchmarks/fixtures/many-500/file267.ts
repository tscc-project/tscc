interface Item267 { id: number; name: string; }
type Key267 = string | number;
export function make267(id: number, name: string): Item267 {
  const value: Item267 = {id, name};
  return value as Item267;
}
export const item267: Item267 = make267(267, "item-267");
