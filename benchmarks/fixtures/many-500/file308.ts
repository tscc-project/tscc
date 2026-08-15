interface Item308 { id: number; name: string; }
type Key308 = string | number;
export function make308(id: number, name: string): Item308 {
  const value: Item308 = {id, name};
  return value as Item308;
}
export const item308: Item308 = make308(308, "item-308");
