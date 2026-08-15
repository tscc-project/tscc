interface Item145 { id: number; name: string; }
type Key145 = string | number;
export function make145(id: number, name: string): Item145 {
  const value: Item145 = {id, name};
  return value as Item145;
}
export const item145: Item145 = make145(145, "item-145");
