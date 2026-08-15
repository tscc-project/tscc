interface Item107 { id: number; name: string; }
type Key107 = string | number;
export function make107(id: number, name: string): Item107 {
  const value: Item107 = {id, name};
  return value as Item107;
}
export const item107: Item107 = make107(107, "item-107");
