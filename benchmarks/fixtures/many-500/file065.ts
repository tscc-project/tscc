interface Item65 { id: number; name: string; }
type Key65 = string | number;
export function make65(id: number, name: string): Item65 {
  const value: Item65 = {id, name};
  return value as Item65;
}
export const item65: Item65 = make65(65, "item-65");
