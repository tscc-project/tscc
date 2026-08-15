interface Item239 { id: number; name: string; }
type Key239 = string | number;
export function make239(id: number, name: string): Item239 {
  const value: Item239 = {id, name};
  return value as Item239;
}
export const item239: Item239 = make239(239, "item-239");
