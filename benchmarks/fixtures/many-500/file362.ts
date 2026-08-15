interface Item362 { id: number; name: string; }
type Key362 = string | number;
export function make362(id: number, name: string): Item362 {
  const value: Item362 = {id, name};
  return value as Item362;
}
export const item362: Item362 = make362(362, "item-362");
